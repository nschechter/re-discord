let extractAuthor = (data: Yojson.Basic.t): Message.author => {
  username:
    data
    |> Yojson.Basic.Util.member("username")
    |> Yojson.Basic.Util.to_string,
  id: data |> Yojson.Basic.Util.member("id") |> Yojson.Basic.Util.to_string,
  discriminator:
    data
    |> Yojson.Basic.Util.member("discriminator")
    |> Yojson.Basic.Util.to_string,
  avatar:
    data |> Yojson.Basic.Util.member("avatar") |> Yojson.Basic.Util.to_string,
};

let extractMessage = (token: string, data: Yojson.Basic.t): Message.t => {
  token,
  id: data |> Yojson.Basic.Util.member("id") |> Yojson.Basic.Util.to_string,
  guildId:
    data
    |> Yojson.Basic.Util.member("guild_id")
    |> Yojson.Basic.Util.to_string,
  content:
    data |> Yojson.Basic.Util.member("content") |> Yojson.Basic.Util.to_string,
  channelId:
    data
    |> Yojson.Basic.Util.member("channel_id")
    |> Yojson.Basic.Util.to_string,
  author: data |> Yojson.Basic.Util.member("author") |> extractAuthor,
  timestamp:
    data
    |> Yojson.Basic.Util.member("timestamp")
    |> Yojson.Basic.Util.to_string,
};

let handle = (token, data, onMessage) => {
  data |> extractMessage(token) |> onMessage;
};