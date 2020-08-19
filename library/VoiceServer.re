open Yojson.Basic.Util;

type t = {
  token: string,
  guildId: string,
  endpoint: string,
};

let extract = data => {
  token: data |> member("token") |> to_string,
  guildId: data |> member("guild_id") |> to_string,
  endpoint: data |> member("endpoint") |> to_string,
};

let handle = payload => {
  payload |> extract;
};
