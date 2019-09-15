open Yojson.Basic.Util;

type t = {
  name: string,
  id: string,
  channels: list(Channel.t),
  members: list(Member.t),
};

let extract = (data: Yojson.Basic.t): t => {
  name: data |> member("name") |> to_string,
  id: data |> member("id") |> to_string,
  channels: data |> member("channels") |> convert_each(Channel.extract),
  members: data |> member("members") |> convert_each(Member.extract),
};