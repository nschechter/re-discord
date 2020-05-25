open Lwt.Infix;
open Websocket;
open Frame;

type state = {
  token: string,
  heartbeatInterval: option(int),
  sessionId: option(string),
  guild: option(Guild.t),
};

let lastSequence = ref(0);

let make =
    (
      ~debug,
      ~onReady,
      ~onMessage,
      ~onGuildMemberAdd,
      ~onGuildMemberRemove,
      ~onReactionAdd,
      ~onReactionRemove,
      ~token,
      uri,
    ) => {
  Client.connect(uri)
  >>= (
    ((recv, send)) => {
      let sendFrame = (frame: Frame.t) => {
        debug ? print_endline("Sending: " ++ frame.content) : ignore();
        send @@ frame;
      };

      let rec triggerHeartbeat = interval => {
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
            triggerHeartbeat(interval);
          },
        )
        |> Lwt_timeout.start;
      };

      let setSequence = num => {
        lastSequence := num;
      };

      let close = () =>
        sendFrame(Frame.close(1002)) >>= (() => Lwt.fail(Exit));

      let handlePayload = (state, payload) => {
        payload
        |> PayloadParser.parse(setSequence)
        |> (
          fun
          | Hello(int) => {
              triggerHeartbeat(int / 1000);
              sendFrame(
                Frame.create(
                  ~opcode=Opcode.Binary,
                  ~content=PayloadGenerator.identify(token),
                  (),
                ),
              )
              |> ignore;
              {
                token,
                heartbeatInterval: Some(int),
                sessionId: None,
                guild: None,
              };
            }
          | Ready(payload) => {
              let readyInfo = ReadyHandler.handle(state, payload.d, onReady);
              {
                token: state.token,
                heartbeatInterval: state.heartbeatInterval,
                sessionId: Some(readyInfo.sessionId),
                guild: None,
              };
            }
          | GuildCreate(payload) => {
              {
                token: state.token,
                heartbeatInterval: state.heartbeatInterval,
                sessionId: state.sessionId,
                guild: Some(Guild.extract(token, payload.d)),
              };
            }
          | GuildMemberAdd(payload) =>
            switch (onGuildMemberAdd, state.guild) {
            | (Some(handler), Some(guild)) =>
              handler(guild, GuildMemberHandler.handle(payload.d));
              {
                token: state.token,
                heartbeatInterval: state.heartbeatInterval,
                sessionId: state.sessionId,
                guild:
                  Some(Guild.addMember(guild, payload.d |> Member.extract)),
              };
            | (_, Some(guild)) => {
                token: state.token,
                heartbeatInterval: state.heartbeatInterval,
                sessionId: state.sessionId,
                guild:
                  Some(Guild.addMember(guild, payload.d |> Member.extract)),
              }
            | (_, _) => state
            }
          | GuildMemberRemove(payload) =>
            switch (onGuildMemberRemove, state.guild) {
            | (Some(handler), Some(guild)) =>
              handler(guild, GuildMemberHandler.handle(payload.d));
              {
                token: state.token,
                heartbeatInterval: state.heartbeatInterval,
                sessionId: state.sessionId,
                guild:
                  Some(
                    Guild.removeMember(guild, payload.d |> Member.extract),
                  ),
              };
            | (_, Some(guild)) => {
                heartbeatInterval: state.heartbeatInterval,
                token: state.token,
                sessionId: state.sessionId,
                guild:
                  Some(
                    Guild.removeMember(guild, payload.d |> Member.extract),
                  ),
              }
            | (_, _) => state
            }
          | ChannelCreate(payload) =>
            switch (state.guild) {
            | Some(guild) => {
                token: state.token,
                heartbeatInterval: state.heartbeatInterval,
                sessionId: state.sessionId,
                guild:
                  Some(
                    Guild.addChannel(
                      guild,
                      payload.d |> Channel.extract(token),
                    ),
                  ),
              }
            | None => state
            }
          | ChannelDelete(payload) =>
            switch (state.guild) {
            | Some(guild) => {
                token: state.token,
                heartbeatInterval: state.heartbeatInterval,
                sessionId: state.sessionId,
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
              switch (onMessage) {
              | Some(handler) =>
                MessageHandler.handle(token, payload.d, handler) |> ignore
              | None => ignore()
              };
              state;
            }
          | MessageReactionRemove(payload) => {
              switch (onReactionRemove) {
              | Some(handler) =>
                MessageHandler.handleReaction(token, payload.d, handler)
              | None => ignore()
              };
              state;
            }
          | MessageReactionAdd(payload) => {
              switch (onReactionAdd) {
              | Some(handler) =>
                MessageHandler.handleReaction(token, payload.d, handler)
              | None => ignore()
              };
              state;
            }
          | PresenceUpdate(_) => state
          | HeartbeatACK
          | Unknown => state
        );
      };

      let handleFrame = (fr: Frame.t, state) => {
        debug
          ? {
            print_endline(
              "Received Opcode: " ++ (fr.opcode |> Opcode.to_string),
            );
            print_endline(
              "Receieved extension: " ++ (fr.extension |> string_of_int),
            );
            print_endline(
              "Receieved final: " ++ (fr.final |> string_of_bool),
            );
            print_endline("Receieved content: " ++ fr.content);
          }
          : ignore();

        (
          switch (fr.opcode) {
          | Opcode.Ping =>
            send @@ Frame.create(~opcode=Opcode.Pong, ()) |> ignore;
            state;
          | Opcode.Text
          | Opcode.Binary => handlePayload(state, fr.content)
          | _ =>
            close() |> ignore;
            state;
          }
        )
        |> Lwt.return;
      };

      let rec react_forever = (state: state) =>
        recv()
        >>= (
          frame =>
            handleFrame(frame, state) >>= (state => react_forever(state))
        );

      react_forever({
        token,
        heartbeatInterval: None,
        sessionId: None,
        guild: None,
      });
    }
  );
};