open Lwt.Infix;

type sessionStartLimit = {
  total: int,
  remaining: int,
  resetAfter: int,
};

type gatewayData = {
  url: string,
  shards: int,
  sessionStartLimit,
};

type gatewayResponse =
  | Success(gatewayData)
  | Error;

let extractSessionStartLimit = (json: Yojson.Basic.t): sessionStartLimit => {
  total:
    json |> Yojson.Basic.Util.member("total") |> Yojson.Basic.Util.to_int,
  remaining:
    json |> Yojson.Basic.Util.member("remaining") |> Yojson.Basic.Util.to_int,
  resetAfter:
    json
    |> Yojson.Basic.Util.member("reset_after")
    |> Yojson.Basic.Util.to_int,
};

let extractGatewayResponse = (json: Yojson.Basic.t): gatewayResponse =>
  Success({
    url:
      json |> Yojson.Basic.Util.member("url") |> Yojson.Basic.Util.to_string,
    shards:
      json |> Yojson.Basic.Util.member("shards") |> Yojson.Basic.Util.to_int,
    sessionStartLimit:
      json
      |> Yojson.Basic.Util.member("session_start_limit")
      |> extractSessionStartLimit,
  });

let extractHeartbeat = (json: Yojson.Basic.t): option(int) =>
  json
  |> Yojson.Basic.Util.member("d")
  |> Yojson.Basic.Util.member("heartbeat_interval")
  |> Yojson.Basic.Util.to_int_option;

let getDiscordGateway = (~token): Lwt.t(gatewayResponse) =>
  Cohttp_lwt_unix.Client.get(
    ~headers=Cohttp.Header.init_with("Authorization", "Bot " ++ token),
    Uri.of_string(
      Constants.http.baseApi
      ++ "/v"
      ++ Constants.http.version
      ++ Constants.endpoints.botGateway,
    ),
  )
  >>= (
    ((resp, body)) => {
      resp
      |> Cohttp.Response.status
      |> Cohttp.Code.code_of_status
      |> Cohttp.Code.is_success
      |> (
        fun
        | false => {
            print_endline("Unable to make gateway connection");
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