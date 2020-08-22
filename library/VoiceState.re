open Yojson.Basic.Util;

type t = {
  guildId: option(string),
  channelId: option(string),
  userId: string,
  sessionId: string,
  deaf: bool,
  mute: bool,
};

let extract = json => {
  guildId: json |> member("guild_id") |> to_string_option,
  channelId: json |> member("channel_id") |> to_string_option,
  userId: json |> member("user_id") |> to_string,
  sessionId: json |> member("session_id") |> to_string,
  deaf: json |> member("deaf") |> to_bool,
  mute: json |> member("mute") |> to_bool,
};
