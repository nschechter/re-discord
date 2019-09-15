open Yojson.Basic.Util;

type t = {
  token: string,
  id: string,
  guildId: string,
  content: string,
  channelId: string,
  author: Member.user,
  timestamp: string,
};

let make = (token, id, guildId, content, channelId, author, timestamp): t => {
  token,
  id,
  guildId,
  content,
  channelId,
  author,
  timestamp,
};

let extract = (token: string, data: Yojson.Basic.t): t => {
  token,
  id: data |> member("id") |> to_string,
  guildId: data |> member("guild_id") |> to_string,
  content: data |> member("content") |> to_string,
  channelId: data |> member("channel_id") |> to_string,
  author: data |> member("author") |> Member.extractUser,
  timestamp: data |> member("timestamp") |> to_string,
};

let reply = (content: string, message: t) => {
  Api.createMessage(message.token, message.channelId, content) |> ignore;
  message;
};

let react = (emoji: string, message: t) => {
  Api.createReact(message.token, message.channelId, message.id, emoji)
  |> ignore;
  message;
};