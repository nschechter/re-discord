open Lwt.Infix;
open Websocket_lwt_unix;

let connect = (uri): Lwt.t('a) =>
  Resolver_lwt.resolve_uri(~uri, Resolver_lwt_unix.system)  // create URI
  >>= (
    endpoint =>
      Conduit_lwt_unix.(
        endp_to_client(~ctx=default_ctx, endpoint)  // configures connection type
        >>= (
          client => {
            with_connection(
              ~ctx=default_ctx,
              client,
              uri // returns input / output channels
            );
          }
        )
      )
  );