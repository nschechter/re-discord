open Yojson.Basic.Util;

type t = {
  name: string,
  id: string,
  channels: list(Channel.t),
  members: list(Member.t),
};

let extract = (token, data: Yojson.Basic.t): t => {
  name: data |> member("name") |> to_string,
  id: data |> member("id") |> to_string,
  channels:
    data |> member("channels") |> convert_each(Channel.extract(token)),
  members: data |> member("members") |> convert_each(Member.extract),
};

let addMember = (guild: t, member: Member.t): t => {
  name: guild.name,
  id: guild.id,
  channels: guild.channels,
  members: List.append(guild.members, [member]),
};

let removeMember = (guild: t, member: Member.t): t => {
  name: guild.name,
  id: guild.id,
  channels: guild.channels,
  members:
    guild.members
    |> List.filter((mem: Member.t) => mem.user.id != member.user.id),
};

let addChannel = (guild: t, channel: Channel.t): t => {
  name: guild.name,
  id: guild.id,
  channels: List.append(guild.channels, [channel]),
  members: guild.members,
};

let deleteChannel = (guild: t, channel: Channel.t): t => {
  name: guild.name,
  id: guild.id,
  channels:
    guild.channels |> List.filter((c: Channel.t) => c.id != channel.id),
  members: guild.members,
};