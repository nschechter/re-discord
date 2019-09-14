open Lwt.Infix;
open Websocket;
open Websocket_lwt_unix;
open Frame;

type heartbeat =
  | Initialized
  | Error
  | AlreadyInitialized;

type state = {
  heartbeatInterval: option(int),
  token: string,
};

let lastSequence = ref(0);

let make = (uri, onMessage, token) => {
  Client.connect(uri)
  >>= (
    ((recv, send)) => {
      print_endline("Connected to server");

      let sendMessage = (frame: Frame.t) => {
        print_endline("Sending: " ++ frame.content);
        send @@ frame;
      };

      let rec triggerHeartbeat = interval => {
        Lwt_timeout.create(
          interval,
          () => {
            sendMessage(
              Frame.create(
                ~opcode=Frame.Opcode.Text,
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
        sendMessage(Frame.close(1002)) >>= (() => Lwt.fail(Exit));

      let handlePayload = (state, payload) => {
        payload
        |> PayloadParser.parse(setSequence)
        |> (
          fun
          | Hello(int) => {
              triggerHeartbeat(int / 1000);
              sendMessage(
                Frame.create(
                  ~opcode=Opcode.Binary,
                  ~content=PayloadGenerator.identify(token),
                  (),
                ),
              )
              |> ignore;
              {heartbeatInterval: Some(int), token};
            }
          | Ready(payload) => state
          | GuildCreate(payload) => state
          | MessageCreate(payload) =>
            switch (onMessage) {
            | Some(handler) =>
              MessageHandler.handle(token, payload.d, handler) |> ignore;
              state;
            | None => state
            }
          | MessageReactionRemove(payload) => state
          | MessageReactionAdd(payload) => state
          | PresenceUpdate(payload) => state
          | HeartbeatACK
          | Unknown => state
        );
      };

      let handleFrame = (fr, state) => {
        print_endline("Received Opcode: " ++ (fr.opcode |> Opcode.to_string));
        print_endline(
          "Receieved extension: " ++ (fr.extension |> string_of_int),
        );
        print_endline("Receieved final: " ++ (fr.final |> string_of_bool));
        print_endline("Receieved content: " ++ fr.content);

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

      react_forever({heartbeatInterval: None, token});
    }
  );
};