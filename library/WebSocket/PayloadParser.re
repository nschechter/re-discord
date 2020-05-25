type response = {
  op: int,
  d: Yojson.Basic.t,
  s: option(int),
  t: option(string),
};

type discordEvent =
  | Hello(int)
  | Ready(response)
  | ChannelCreate(response)
  | ChannelDelete(response)
  | GuildCreate(response)
  | GuildMemberAdd(response)
  | GuildMemberRemove(response)
  | MessageCreate(response)
  | MessageReactionAdd(response)
  | MessageReactionRemove(response)
  | PresenceUpdate(response)
  | HeartbeatACK
  | Unknown;

let parseEvent = response => {
  switch (response.op, response.t) {
  | (0, Some(t)) =>
    switch (t) {
    | "READY" => Ready(response)
    | "CHANNEL_CREATE" => ChannelCreate(response)
    | "CHANNEL_DELETE" => ChannelDelete(response)
    | "GUILD_CREATE" => GuildCreate(response)
    | "GUILD_MEMBER_ADD" => GuildMemberAdd(response)
    | "GUILD_MEMBER_REMOVE" => GuildMemberRemove(response)
    | "PRESENCE_UPDATE" => PresenceUpdate(response)
    | "MESSAGE_CREATE" => MessageCreate(response)
    | "MESSAGE_REACTION_ADD" => MessageReactionAdd(response)
    | "MESSAGE_REACTION_REMOVE" => MessageReactionRemove(response)
    | _ =>
      print_endline("Received unsupported type: " ++ t);
      Unknown;
    }
  | (10, None) =>
    Hello(
      response.d
      |> Yojson.Basic.Util.member("heartbeat_interval")
      |> Yojson.Basic.Util.to_int,
    )
  | (11, _) => HeartbeatACK
  | (_, _) => Unknown
  };
};

let extractSequence = (setSequence, response) => {
  switch (response.s) {
  | Some(num) => setSequence(num)
  | None => ignore()
  };
  response;
};

let extractPayload = response => {
  op:
    response
    |> Yojson.Basic.from_string
    |> Yojson.Basic.Util.member("op")
    |> Yojson.Basic.Util.to_int,
  d: response |> Yojson.Basic.from_string |> Yojson.Basic.Util.member("d"),
  s:
    response
    |> Yojson.Basic.from_string
    |> Yojson.Basic.Util.member("s")
    |> Yojson.Basic.Util.to_int_option,
  t:
    response
    |> Yojson.Basic.from_string
    |> Yojson.Basic.Util.member("t")
    |> Yojson.Basic.Util.to_string_option,
};

let parse = (setSequence, message) => {
  message |> extractPayload |> extractSequence(setSequence) |> parseEvent;
};