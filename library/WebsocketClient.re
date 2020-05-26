open Lwt.Infix;
open Websocket.Frame;

let sendFrame = (~debug, ~send, frame) => {
  debug ? print_endline("Sending: " ++ frame.content) : ignore();
  send @@ frame;
};

let printFrame = frame => {
  print_endline("Received Opcode: " ++ (frame.opcode |> Opcode.to_string));
  print_endline(
    "Receieved extension: " ++ (frame.extension |> string_of_int),
  );
  print_endline("Receieved final: " ++ (frame.final |> string_of_bool));
  print_endline("Receieved content: " ++ frame.content);
};

let connect = (~debug, ~initialState, ~handleFrame, uri): Lwt.t('a) =>
  Resolver_lwt.resolve_uri(~uri, Resolver_lwt_unix.system)
  >>= (
    endpoint =>
      Conduit_lwt_unix.(
        endp_to_client(~ctx=default_ctx, endpoint)
        >>= (
          client =>
            Websocket_lwt_unix.with_connection(~ctx=default_ctx, client, uri)
        )
      )
  )
  >>= (
    ((recv, send)) => {
      let rec listen = state =>
        recv()
        >>= (
          frame =>
            {
              debug ? printFrame(frame) : ignore();
              handleFrame(frame, sendFrame(~debug, ~send), state);
            }
            >>= (state => listen(state))
        );

      listen(initialState);
    }
  );
