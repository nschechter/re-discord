open ReDiscord;

let token = Sys.getenv_opt("DISCORD_BOT_TOKEN");

let onMessage = (message: Message.t) => {
  switch (message.content, token) {
  | ("ping", Some(token)) =>
    message
    |> Message.reply(~token, "pong")
    |> Message.react(~token, "👍")
    |> ignore
  | (_, _) => ignore()
  };
};

let onReady = () => {
  print_endline("Connected!");
};

switch (token) {
| Some(token) => Discord.make(~onReady, ~onMessage, token) |> Lwt_main.run
| None =>
  print_endline("ERROR: No token found, try exporting DISCORD_BOT_TOKEN")
};