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