open Yojson.Basic.Util;

type t = {
  type_: int,
  id: string,
  name: string,
  lastMessageId: option(string),
  parentId: option(string),
};

let extract = (data: Yojson.Basic.t): t => {
  type_: data |> member("type") |> to_int,
  id: data |> member("id") |> to_string,
  name: data |> member("name") |> to_string,
  lastMessageId: data |> member("last_message_id") |> to_string_option,
  parentId: data |> member("parent_id") |> to_string_option,
};

let send = (~token: string, ~channel: t, content: string) => {
  Api.createMessage(~token, ~channelId=channel.id, content) |> ignore;
};