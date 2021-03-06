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