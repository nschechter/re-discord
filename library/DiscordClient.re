open Lwt.Infix;
open Websocket;
open Frame;

type state = {
  token: string,
  heartbeatInterval: option(int),
  sessionId: option(string),
  guilds: Guild.Map.t(Guild.t),
  botUser: option(Member.user),
  isInVoiceChannel: bool,
};

let lastSequence = ref(0);

let rec triggerHeartbeat = (interval, sendFrame) =>
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

let setSequence = num => lastSequence := num;

let voiceConnect = (~sendFrame, ~guildId, ~channelId) =>
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
    ) =>
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
        let guild = Guild.extract(token, payload.d);
        let newGuildsMap = Guild.Map.add(guild.id, guild, state.guilds);

        {...state, guilds: newGuildsMap};
      }
    | GuildMemberAdd(payload) => {
        let member = payload.d |> GuildMember.extract;

        switch (member.guildId) {
        | Some(guildId) =>
          Guild.Map.find_opt(guildId, state.guilds)
          |> (
            fun
            | Some(guild) => {
                let newGuild = Guild.addMember(guild, member);

                let newGuildsMap =
                  Guild.Map.remove(guild.id, state.guilds)
                  |> Guild.Map.add(newGuild.id, newGuild);

                switch (onGuildMemberAdd) {
                | Some(handler) => handler(newGuild, member)
                | None => ignore()
                };

                {...state, guilds: newGuildsMap};
              }
            | None => state
          )
        | None => state
        };
      }
    | GuildMemberRemove(payload) => {
        let member = payload.d |> GuildMember.extract;

        switch (member.guildId) {
        | Some(guildId) =>
          Guild.Map.find_opt(guildId, state.guilds)
          |> (
            fun
            | Some(guild) => {
                let newGuild = Guild.removeMember(guild, member);

                let newGuildsMap =
                  Guild.Map.remove(guild.id, state.guilds)
                  |> Guild.Map.add(newGuild.id, newGuild);

                switch (onGuildMemberRemove) {
                | Some(handler) => handler(newGuild, member)
                | None => ignore()
                };

                {...state, guilds: newGuildsMap};
              }
            | None => state
          )
        | None => state
        };
      }
    | ChannelCreate(payload) => {
        let channel = payload.d |> Channel.extract(token);

        channel.guildId
        |> (
          fun
          | Some(guildId) => {
              Guild.Map.find_opt(guildId, state.guilds)
              |> (
                fun
                | Some(guild) => {
                    let newGuild = Guild.addChannel(guild, channel);
                    let newGuildsMap =
                      Guild.Map.remove(guild.id, state.guilds)
                      |> Guild.Map.add(newGuild.id, newGuild);

                    {...state, guilds: newGuildsMap};
                  }
                | None => state
              );
            }
          | None => state
        );
      }
    | ChannelDelete(payload) => {
        let channel = payload.d |> Channel.extract(token);

        channel.guildId
        |> (
          fun
          | Some(guildId) => {
              Guild.Map.find_opt(guildId, state.guilds)
              |> (
                fun
                | Some(guild) => {
                    let newGuild = Guild.removeChannel(guild, channel);
                    let newGuildsMap =
                      Guild.Map.remove(guild.id, state.guilds)
                      |> Guild.Map.add(newGuild.id, newGuild);

                    {...state, guilds: newGuildsMap};
                  }
                | None => state
              );
            }
          | None => state
        );
      }
    | MessageCreate(payload) => {
        let message = Message.extract(token, payload.d);

        switch (state.botUser, message.guildId) {
        | (Some(botUser), Some(guildId)) =>
          message.author.id == botUser.id
            ? ignore()
            : {
              switch (onMessage) {
              | Some(handler) =>
                Message.handle(token, payload.d, handler) |> ignore
              | None => ignore()
              };

              switch (
                onMessageWithVoice,
                Guild.Map.find_opt(guildId, state.guilds),
                state.isInVoiceChannel,
              ) {
              | (Some(handler), Some(guild), false) =>
                List.find_opt(
                  (vs: VoiceState.t) => vs.userId == message.author.id,
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
        | (_, _) => ignore()
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
        let voiceState = payload.d |> VoiceState.extract;
        switch (voiceState.guildId, state.botUser) {
        | (Some(guildId), Some(user)) =>
          Guild.Map.find_opt(guildId, state.guilds)
          |> (
            fun
            | Some(guild) => {
                let newGuild = Guild.updateVoiceState(guild, voiceState);
                let newGuildsMap =
                  Guild.Map.remove(guild.id, state.guilds)
                  |> Guild.Map.add(newGuild.id, newGuild);

                {
                  ...state,
                  isInVoiceChannel:
                    voiceState.userId == user.id
                    && voiceState.channelId == None
                      ? false : state.isInVoiceChannel,
                  guilds: newGuildsMap,
                };
              }
            | None => state
          )
        | (_, _) => state
        };
      }
    | VoiceServerUpdate(payload) => {
        let voiceServer = VoiceServer.handle(payload.d);

        let endpoint =
          String.split_on_char(':', voiceServer.endpoint) |> List.hd;

        switch (
          state.botUser,
          Guild.Map.find_opt(voiceServer.guildId, state.guilds),
          state.sessionId,
        ) {
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
    | HeartbeatACK
    | _ => state
  );

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
      guilds: Guild.Map.empty,
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
