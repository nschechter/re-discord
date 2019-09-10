open Lwt.Infix;
open Websocket;
open Websocket_lwt_unix;

let connect = (uri): Lwt.t('a) =>
  Frame.(
    Resolver_lwt.resolve_uri(~uri, Resolver_lwt_unix.system)  // create URI
    >>= (
      endp =>
        Conduit_lwt_unix.(
          endp_to_client(~ctx=default_ctx, endp)  // configures connection type
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
    )
  );