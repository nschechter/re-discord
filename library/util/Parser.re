type response = {
  op: int,
  d: Yojson.Basic.t,
  s: option(int),
  t: option(string),
};

type discordEvent =
  | Ready(Yojson.Basic.t)
  | GuildCreate(Yojson.Basic.t)
  | MessageCreate(Yojson.Basic.t)
  | PresenceUpdate(Yojson.Basic.t)
  | MessageReactionAdd(Yojson.Basic.t)
  | MessageReactionRemove(Yojson.Basic.t)
  | HeartbeatACK
  | Unknown;

let extractEvent = response => {
  switch (response.op, response.t) {
  | (0, Some(t)) =>
    switch (t) {
    | "READY" => Ready(response.d)
    | "GUILD_CREATE" => GuildCreate(response.d)
    | "MESSAGE_CREATE" => MessageCreate(response.d)
    | "PRESENCE_UPDATE" => PresenceUpdate(response.d)
    | "MESSAGE_REACTION_ADD" => MessageReactionAdd(response.d)
    | "MESSAGE_REACTION_REMOVE" => MessageReactionRemove(response.d)
    | _ =>
      print_endline("Received unsupported type: " ++ t);
      Unknown;
    }
  | (11, _) => HeartbeatACK
  | (_, _) => Unknown
  };
};

let extractSequence = (setSequence, response) => {
  switch (response.s) {
  | Some(num) =>
    print_endline("Extracted sequence: " ++ string_of_int(num));
    setSequence(num);
  | None => print_endline("No sequence?")
  };
  response;
};

let extractData = response => {
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
  message |> extractData |> extractSequence(setSequence) |> extractEvent;
};