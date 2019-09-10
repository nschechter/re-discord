open Lwt.Infix;
open Websocket;
open Websocket_lwt_unix;
open Frame;

type heartbeat =
  | Initialized
  | Error
  | AlreadyInitialized;

type state = {
  heartbeat: int,
  sequence: int,
};

let heartbeat = ref(-1);
let lastSequence = ref(0);

let setHeartbeat = message => {
  heartbeat^ < 0
    ? message
      |> Yojson.Basic.from_string
      |> Util.extractHeartbeat
      |> (
        fun
        | Some(num) => {
            print_endline("Decoded heartbeat: " ++ string_of_int(num));
            heartbeat := num / 1000;
            Initialized;
          }
        | None => Error
      )
    : AlreadyInitialized;
};

let make = (uri, onMessageHandler, token) => {
  Client.connect(uri)
  >>= (
    ((recv, send)) => {
      print_endline("Connected to server");

      let rec triggerHeartbeat = () => {
        Lwt_timeout.create(
          heartbeat^,
          () => {
            print_endline(
              "Sending heartbeat payload: "
              ++ Payload.heartbeat(lastSequence^),
            );
            send @@
            Frame.create(
              ~opcode=Frame.Opcode.Text,
              ~content=Payload.heartbeat(lastSequence^),
              (),
            )
            |> ignore;
            triggerHeartbeat();
          },
        )
        |> Lwt_timeout.start;
      };

      let sendMessage = (frame: Frame.t) => {
        send @@ frame;
      };

      let setSequence = num => {
        lastSequence := num;
      };

      let close = () => send @@ Frame.close(1002) >>= (() => Lwt.fail(Exit));

      let handleMessage = message => {
        setHeartbeat(message)
        |> (
          fun
          | Initialized => {
              triggerHeartbeat();
              print_endline(
                "Sending identify payload: " ++ Payload.identify(token),
              );
              sendMessage(
                Frame.create(
                  ~opcode=Opcode.Binary,
                  ~content=Payload.identify(token),
                  (),
                ),
              )
              |> ignore;
            }
          | AlreadyInitialized =>
            message
            |> Parser.parse(setSequence)
            |> (
              fun
              | Ready(data) => ignore()
              | GuildCreate(data) => ignore()
              | MessageCreate(data) =>
                switch (onMessageHandler) {
                | Some(handler) =>
                  MessageHandler.handle(token, data, handler) |> ignore
                | None => ignore()
                }
              | MessageReactionRemove(data) => ignore()
              | MessageReactionAdd(data) => ignore()
              | PresenceUpdate(data) => ignore()
              | Unknown => ignore()
              | HeartbeatACK => ignore()
            )
          | Error => {
              print_endline("Unknown message, closing: " ++ message);
              close() |> ignore;
            }
        );
        Lwt.return_unit;
      };

      let handleFrame = (fr, state) => {
        print_endline("Received Opcode: " ++ (fr.opcode |> Opcode.to_string));
        print_endline(
          "Receieved extension: " ++ (fr.extension |> string_of_int),
        );
        print_endline("Receieved final: " ++ (fr.final |> string_of_bool));
        print_endline("Receieved content: " ++ fr.content);
        switch (fr.opcode) {
        | Opcode.Ping =>
          send @@ Frame.create(~opcode=Opcode.Pong, ()) |> ignore
        | Opcode.Text
        | Opcode.Binary => handleMessage(fr.content) |> ignore
        | _ => close() |> ignore
        };

        switch (state) {
        | Some(state) => Lwt.return(Some(state))
        | None => Lwt.return(None)
        };
      };

      let rec react_forever = state =>
        recv()
        >>= (
          frame =>
            handleFrame(frame, state) >>= (state => react_forever(state))
        );
      react_forever(None);
    }
  );
};