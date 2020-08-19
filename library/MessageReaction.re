open Yojson.Basic.Util;

type emoji = {
  name: string,
  id: option(string),
  animated: bool,
};

type t = {
  token: string,
  userId: string,
  messageId: string,
  emoji,
  channelId: string,
  guildId: string,
};

let extractEmoji = (data: Yojson.Basic.t): emoji => {
  name: data |> member("user_id") |> to_string,
  id: data |> member("user_id") |> to_string_option,
  animated: data |> member("user_id") |> to_bool,
};

let extract = (token: string, data: Yojson.Basic.t): t => {
  token,
  userId: data |> member("user_id") |> to_string,
  messageId: data |> member("message_id") |> to_string,
  emoji: data |> member("emoji") |> extractEmoji,
  channelId: data |> member("channel_id") |> to_string,
  guildId: data |> member("guild_id") |> to_string,
};

let handle = (token, data, handler) => {
  data |> extract(token) |> handler;
};
