open Lwt.Infix;
open Websocket;
open Frame;

type state = {
  token: string,
  heartbeatInterval: option(int),
  sessionId: option(string),
  guild: option(Guild.t),
  botUser: option(Member.user),
  isInVoiceChannel: bool,
};

let lastSequence = ref(0);

let rec triggerHeartbeat = (interval, sendFrame) => {
  Lwt_timeout.create(
    interval,
    () => {
      sendFrame(
        Frame.create(
          ~opcode=Opcode.Text,
          ~content=PayloadGenerator.heartbeat(lastSequence^),
          (),
        ),
      )
      |> ignore;
      triggerHeartbeat(interval, sendFrame);
    },
  )
  |> Lwt_timeout.start;
};

let setSequence = num => {
  lastSequence := num;
};

let voiceConnect = (~sendFrame, ~guildId, ~channelId) => {
  switch (channelId) {
  | Some(id) =>
    Frame.create(
      ~opcode=Opcode.Text,
      ~content=PayloadGenerator.voiceState(~guildId, ~channelId=id),
      (),
    )
    |> sendFrame
    |> ignore
  | None => ignore()
  };
};

let handleEvent =
    (
      ~debug,
      ~token,
      ~state,
      ~sendFrame,
      ~onReady,
      ~onMessage,
      ~onMessageWithVoice,
      ~onGuildMemberAdd,
      ~onGuildMemberRemove,
      ~onReactionAdd,
      ~onReactionRemove,
      ~onVoiceConnect,
      payload,
    ) => {
  payload
  |> PayloadParser.parse(setSequence)
  |> (
    fun
    | Hello(int) => {
        triggerHeartbeat(int / 1000, sendFrame);
        sendFrame(
          Frame.create(
            ~opcode=Opcode.Binary,
            ~content=PayloadGenerator.identify(token),
            (),
          ),
        )
        |> ignore;
        state;
      }
    | Ready(payload) => {
        let readyData = Ready.handle(state, payload.d, onReady);

        {
          ...state,
          sessionId: Some(readyData.sessionId),
          botUser: Some(readyData.user),
        };
      }
    | GuildCreate(payload) => {
        {...state, guild: Some(Guild.extract(token, payload.d))};
      }
    | GuildMemberAdd(payload) =>
      switch (onGuildMemberAdd, state.guild) {
      | (Some(handler), Some(guild)) =>
        payload.d |> GuildMember.extract |> handler(guild);
        {
          ...state,
          guild: Some(Guild.addMember(guild, payload.d |> Member.extract)),
        };
      | (_, Some(guild)) => {
          ...state,
          guild: Some(Guild.addMember(guild, payload.d |> Member.extract)),
        }
      | (_, _) => state
      }
    | GuildMemberRemove(payload) =>
      // GuildMember.onMemberRemove(onGuildMemberRemove, payload, state)
      switch (onGuildMemberRemove, state.guild) {
      | (Some(handler), Some(guild)) =>
        payload.d |> GuildMember.extract |> handler(guild);
        {
          ...state,
          guild:
            Some(Guild.removeMember(guild, payload.d |> Member.extract)),
        };
      | (_, Some(guild)) => {
          ...state,
          guild:
            Some(Guild.removeMember(guild, payload.d |> Member.extract)),
        }
      | (_, _) => state
      }
    | ChannelCreate(payload) =>
      switch (state.guild) {
      | Some(guild) => {
          ...state,
          guild:
            Some(
              Guild.addChannel(guild, payload.d |> Channel.extract(token)),
            ),
        }
      | None => state
      }
    | ChannelDelete(payload) =>
      switch (state.guild) {
      | Some(guild) => {
          ...state,
          guild:
            Some(
              Guild.deleteChannel(
                guild,
                payload.d |> Channel.extract(token),
              ),
            ),
        }
      | None => state
      }
    | MessageCreate(payload) => {
        let userId = Message.extract(token, payload.d).author.id;

        switch (state.botUser) {
        | Some(botUser) =>
          userId == botUser.id
            ? ignore()
            : {
              switch (onMessage) {
              | Some(handler) =>
                Message.handle(token, payload.d, handler) |> ignore
              | None => ignore()
              };

              switch (onMessageWithVoice, state.guild, state.isInVoiceChannel) {
              | (Some(handler), Some(guild), false) =>
                List.find_opt(
                  (vs: VoiceState.t) => vs.userId == userId,
                  guild.voiceStates,
                )
                |> (
                  fun
                  | Some(voiceState) => {
                      voiceConnect(
                        ~sendFrame,
                        ~guildId=guild.id,
                        ~channelId=voiceState.channelId,
                      );
                      Message.handleWithVoice(token, payload.d, handler);
                    }
                  | None =>
                    Message.reply(
                      "You are not in a voice channel!",
                      Message.extract(token, payload.d),
                    )
                    |> ignore
                )
              | (_, _, _) => ignore()
              };
            }
        | None => ignore()
        };

        state;
      }
    | MessageReactionRemove(payload) => {
        switch (onReactionRemove) {
        | Some(handler) => MessageReaction.handle(token, payload.d, handler)
        | None => ignore()
        };
        state;
      }
    | MessageReactionAdd(payload) => {
        switch (onReactionAdd) {
        | Some(handler) => MessageReaction.handle(token, payload.d, handler)
        | None => ignore()
        };
        state;
      }
    | VoiceStateUpdate(payload) => {
        switch (state.guild, state.botUser) {
        | (Some(guild), Some(user)) =>
          let voiceState = payload.d |> VoiceState.extract;

          {
            ...state,
            isInVoiceChannel:
              voiceState.userId == user.id && voiceState.channelId == None
                ? false : state.isInVoiceChannel,
            guild: Some(Guild.updateVoiceState(guild, voiceState)),
          };
        | (_, _) => state
        };
      }
    | VoiceServerUpdate(payload) => {
        let voiceServer = VoiceServer.handle(payload.d);

        let endpoint =
          String.split_on_char(':', voiceServer.endpoint) |> List.hd;

        switch (state.botUser, state.guild, state.sessionId) {
        | (Some(user), Some(guild), Some(sessionId)) =>
          VoiceClient.connect(
            ~debug,
            ~onVoiceConnect,
            ~token=voiceServer.token,
            ~serverId=guild.id,
            ~sessionId,
            ~userId=user.id,
            Uri.add_query_param(
              Uri.with_scheme(
                Uri.of_string("//" ++ endpoint),
                Some("https"),
              ),
              ("v", ["4"]),
            ),
          )
          |> ignore
        | (_, _, _) => ignore()
        };

        {...state, isInVoiceChannel: true};
      }
    | HeartbeatACK => {
        state;
      }
    | _ => state
  );
};

let connect =
    (
      ~debug,
      ~onReady,
      ~onMessage,
      ~onMessageWithVoice,
      ~onGuildMemberAdd,
      ~onGuildMemberRemove,
      ~onReactionAdd,
      ~onReactionRemove,
      ~onVoiceConnect,
      ~token,
      uri,
    ) =>
  WebsocketClient.connect(
    ~debug,
    ~initialState={
      token,
      heartbeatInterval: None,
      sessionId: None,
      guild: None,
      botUser: None,
      isInVoiceChannel: false,
    },
    ~handleFrame=
      (frame, sendFrame, state) =>
        (
          switch (frame.opcode) {
          | Opcode.Ping =>
            sendFrame(Frame.create(~opcode=Opcode.Pong, ())) |> ignore;
            state;
          | Opcode.Text
          | Opcode.Binary =>
            handleEvent(
              ~onReady,
              ~onMessage,
              ~onMessageWithVoice,
              ~onGuildMemberAdd,
              ~onGuildMemberRemove,
              ~onReactionAdd,
              ~onReactionRemove,
              ~onVoiceConnect,
              ~token,
              ~sendFrame,
              ~state,
              ~debug,
              frame.content,
            )
          | _ =>
            sendFrame(Frame.close(1002))
            >>= (() => Lwt.fail(Exit))
            |> ignore;
            state;
          }
        )
        |> Lwt.return,
    uri,
  );
