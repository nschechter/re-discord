open Yojson.Basic.Util;

type t = {
  guildId: option(string),
  user: Member.user,
};

let extract = data => {
  guildId: data |> member("guild_id") |> to_string_option,
  user: data |> member("user") |> Member.extractUser,
};
