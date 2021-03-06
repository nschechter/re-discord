open Yojson.Basic.Util;

module Map =
  Map.Make({
    type t = string;
    let compare = compare;
  });

type t = {
  name: string,
  id: string,
  channels: list(Channel.t),
  members: list(GuildMember.t),
  voiceStates: list(VoiceState.t),
};

let extract = (token, data): t => {
  name: data |> member("name") |> to_string,
  id: data |> member("id") |> to_string,
  channels:
    data |> member("channels") |> convert_each(Channel.extract(token)),
  members: data |> member("members") |> convert_each(GuildMember.extract),
  voiceStates:
    data |> member("voice_states") |> convert_each(VoiceState.extract),
};

let addMember = (guild, member): t => {
  ...guild,
  members: List.append(guild.members, [member]),
};

let removeMember = (guild, member: GuildMember.t): t => {
  ...guild,
  members:
    guild.members
    |> List.filter((mem: GuildMember.t) => mem.user.id != member.user.id),
};

let addChannel = (guild, channel): t => {
  ...guild,
  channels: List.append(guild.channels, [channel]),
};

let removeChannel = (guild, channel: Channel.t): t => {
  ...guild,
  channels:
    guild.channels |> List.filter((c: Channel.t) => c.id != channel.id),
};

let updateVoiceState = (guild, voiceState: VoiceState.t) => {
  ...guild,
  voiceStates:
    List.append(
      List.filter(
        (vs: VoiceState.t) => vs.userId != voiceState.userId,
        guild.voiceStates,
      ),
      [voiceState],
    ),
};
