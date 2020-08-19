open Yojson.Basic.Util;

type t = {
  token: string,
  id: string,
  guildId: option(string),
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

let extract = (token, data: Yojson.Basic.t): t => {
  token,
  id: data |> member("id") |> to_string,
  guildId: data |> member("guild_id") |> to_string_option,
  content: data |> member("content") |> to_string,
  channelId: data |> member("channel_id") |> to_string,
  author: data |> member("author") |> Member.extractUser,
  timestamp: data |> member("timestamp") |> to_string,
};

let reply = (content: string, message: t) => {
  Api.createMessage(
    ~token=message.token,
    ~channelId=message.channelId,
    content,
  )
  |> ignore;

  message;
};

let react = (emoji: string, message: t) => {
  Api.createReact(
    ~token=message.token,
    ~channelId=message.channelId,
    ~messageId=message.id,
    emoji,
  )
  |> ignore;
  message;
};

let handle = (token, data, onMessage) => {
  let message = data |> extract(token);
  onMessage(message);
};

let handleWithVoice = (token, data, onMessageWithVoice) => {
  let message = data |> extract(token);
  onMessageWithVoice(message);
};
