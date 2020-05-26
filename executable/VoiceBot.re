open ReDiscord;

let token = Sys.getenv_opt("DISCORD_BOT_TOKEN");

let onMessageWithVoice = (message: Message.t) => {
  let args = message.content |> String.split_on_char(' ');
  switch (List.nth_opt(args, 0)) {
  | Some("!p") =>
    List.nth_opt(args, 1)
    |> (
      fun
      | Some(url) => {
          message |> Message.reply(url);
        }
      | None => message |> Message.reply("Invalid URL!")
    )
    |> ignore
  | _ => ignore()
  };
};

let onReady = () => {
  print_endline("Connected!");
};

switch (token) {
| Some(token) =>
  Discord.make(~onReady, ~onMessageWithVoice, token) |> Lwt_main.run
| None =>
  print_endline("ERROR: No token found, try exporting DISCORD_BOT_TOKEN")
};
