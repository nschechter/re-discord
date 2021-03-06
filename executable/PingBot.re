open ReDiscord;

let token = Sys.getenv_opt("DISCORD_BOT_TOKEN");

let onMessage = (message: Message.t) => {
  switch (message.content) {
  | "ping" =>
    message |> Message.reply("pong") |> Message.react("👍") |> ignore
  | _ => ignore()
  };
};

let onReady = () => {
  print_endline("Connected!");
};

switch (token) {
| Some(token) =>
  Discord.make(~debug=false, ~onReady, ~onMessage, token) |> Lwt_main.run
| None =>
  print_endline("ERROR: No token found, try exporting DISCORD_BOT_TOKEN")
};
