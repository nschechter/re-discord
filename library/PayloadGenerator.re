let heartbeat = sequence =>
  `Assoc([("op", `Int(1)), ("d", `Int(sequence))])
  |> Yojson.Basic.pretty_to_string;

let identify = token =>
  `Assoc([
    ("op", `Int(2)),
    (
      "d",
      `Assoc([
        ("token", `String(token)),
        (
          "properties",
          `Assoc([
            ("$os", `String("linux")),
            ("$browser", `String("re-discord")),
            ("$device", `String("re-discord")),
          ]),
        ),
      ]),
    ),
  ])
  |> Yojson.Basic.pretty_to_string;

let voiceState = (~guildId, ~channelId) => {
  `Assoc([
    ("op", `Int(4)),
    (
      "d",
      `Assoc([
        ("guild_id", `String(guildId)),
        ("channel_id", `String(channelId)),
        ("self_mute", `Bool(false)),
        ("self_deaf", `Bool(false)),
      ]),
    ),
  ])
  |> Yojson.Basic.pretty_to_string;
};

let voiceIdentify = (~serverId, ~userId, ~sessionId, token) =>
  `Assoc([
    ("op", `Int(0)),
    (
      "d",
      `Assoc([
        ("server_id", `String(serverId)),
        ("user_id", `String(userId)),
        ("session_id", `String(sessionId)),
        ("token", `String(token)),
      ]),
    ),
  ])
  |> Yojson.Basic.pretty_to_string;

let voiceHeartbeat = nonce =>
  `Assoc([("op", `Int(3)), ("d", `Int(nonce))])
  |> Yojson.Basic.pretty_to_string;

let voiceSelect = (~protocol, ~ipAddress, ~port, mode) =>
  `Assoc([
    ("op", `Int(1)),
    (
      "d",
      `Assoc([
        ("protocol", `String(protocol)),
        (
          "data",
          `Assoc([
            ("address", `String(ipAddress)),
            ("port", `String(port)),
            ("mode", `String(mode)),
          ]),
        ),
      ]),
    ),
  ])
  |> Yojson.Basic.pretty_to_string;

let voiceSpeak = (~speaking, ~delay=0, ssrc) =>
  `Assoc([
    ("op", `Int(5)),
    (
      "d",
      `Assoc([
        ("speaking", `Int(speaking)),
        ("delay", `Int(delay)),
        ("ssrc", `Int(ssrc)),
      ]),
    ),
  ])
  |> Yojson.Basic.pretty_to_string;
