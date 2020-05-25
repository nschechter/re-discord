open Lwt.Infix;
open JsonParser;

let _VERSION = "v7";
let _BASE = "https://discordapp.com/api";

let encodeMessage = message =>
  `Assoc([("content", `String(message)), ("tts", `Bool(false))])
  |> Yojson.Basic.to_string;

let get = (~token, uri) =>
  Cohttp_lwt_unix.Client.get(
    ~headers=Cohttp.Header.init_with("Authorization", "Bot " ++ token),
    Uri.of_string(uri),
  );

let post = (~token, ~body, uri) =>
  Cohttp_lwt_unix.Client.post(
    ~headers=
      Cohttp.Header.of_list([
        ("Authorization", "Bot " ++ token),
        ("Content-Type", "application/json"),
      ]),
    ~body=body |> Cohttp_lwt__.Body.of_string,
    Uri.of_string(uri),
  );

let put = (~token, uri) =>
  Cohttp_lwt_unix.Client.put(
    ~headers=Cohttp.Header.init_with("Authorization", "Bot " ++ token),
    Uri.of_string(uri),
  );

let getDiscordGateway = (~token): Lwt.t(gatewayResponse) =>
  get(~token, _BASE ++ "/" ++ _VERSION ++ "/gateway/bot")
  >>= (
    ((resp, body)) => {
      resp
      |> Cohttp.Response.status
      |> Cohttp.Code.code_of_status
      |> Cohttp.Code.is_success
      |> (
        fun
        | false => {
            print_endline("ERROR: Unable to make gateway connection");
            Lwt.return(Error);
          }
        | true =>
          Cohttp_lwt__.Body.to_string(body)
          >>= (
            body => {
              body
              |> Yojson.Basic.from_string
              |> extractGatewayResponse
              |> Lwt.return;
            }
          )
      );
    }
  );

let createMessage = (~token: string, ~channelId: string, message: string) =>
  post(
    ~token,
    ~body=message |> encodeMessage,
    _BASE ++ "/" ++ _VERSION ++ "/channels/" ++ channelId ++ "/messages",
  )
  >>= (
    ((_resp, body)) => {
      Cohttp_lwt__.Body.to_string(body) >>= (body => Lwt.return(body));
    }
  );

let createReact =
    (~token: string, ~channelId: string, ~messageId: string, emoji: string) =>
  put(
    ~token,
    _BASE
    ++ "/"
    ++ _VERSION
    ++ "/channels/"
    ++ channelId
    ++ "/messages/"
    ++ messageId
    ++ "/reactions/"
    ++ emoji
    ++ "/@me",
  )
  >>= (
    ((_resp, body)) => {
      Cohttp_lwt__.Body.to_string(body) >>= (body => Lwt.return(body));
    }
  );