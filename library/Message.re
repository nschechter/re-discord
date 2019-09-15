open Yojson.Basic.Util;

type t = {
  id: string,
  guildId: string,
  content: string,
  channelId: string,
  author: Member.user,
  timestamp: string,
};

let make = (id, guildId, content, channelId, author, timestamp): t => {
  id,
  guildId,
  content,
  channelId,
  author,
  timestamp,
};

let extract = (data: Yojson.Basic.t): t => {
  id: data |> member("id") |> to_string,
  guildId: data |> member("guild_id") |> to_string,
  content: data |> member("content") |> to_string,
  channelId: data |> member("channel_id") |> to_string,
  author: data |> member("author") |> Member.extractUser,
  timestamp: data |> member("timestamp") |> to_string,
};

let reply = (~token: string, content: string, message: t) => {
  Api.createMessage(~token, ~channelId=message.channelId, content) |> ignore;
  message;
};

let react = (~token: string, emoji: string, message: t) => {
  Api.createReact(
    ~token,
    ~channelId=message.channelId,
    ~messageId=message.id,
    emoji,
  )
  |> ignore;
  message;
};