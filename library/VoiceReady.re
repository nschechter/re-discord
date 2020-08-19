open Yojson.Basic.Util;

type t = {
  ssrc: int,
  ip: string,
  port: int,
  modes: list(string),
  heartbeatInterval: int,
};

let extract = json => {
  ssrc: json |> member("ssrc") |> to_int,
  ip: json |> member("ip") |> to_string,
  port: json |> member("port") |> to_int,
  modes: json |> member("modes") |> convert_each(to_string),
  heartbeatInterval: json |> member("heartbeat_interval") |> to_int,
};

let handle = payload => payload |> extract;
