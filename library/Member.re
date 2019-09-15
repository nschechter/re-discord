open Yojson.Basic.Util;

type user = {
  username: string,
  id: string,
  discriminator: string,
  avatar: string,
};

type t = {
  user,
  guildId: string,
};

let extractUser = (data: Yojson.Basic.t): user => {
  username: data |> member("username") |> to_string,
  id: data |> member("id") |> to_string,
  discriminator: data |> member("discriminator") |> to_string,
  avatar: data |> member("avatar") |> to_string,
};

let extract = (data: Yojson.Basic.t): t => {
  user: data |> member("user") |> extractUser,
  guildId: data |> member("guild_id") |> to_string,
};