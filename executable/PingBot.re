open Lwt.Infix;
open ReDiscord;

let token = Sys.getenv_opt("DISCORD_BOT_TOKEN");

let onMessage = (message: Message.t) => {
  switch (message.content) {
  | "ping" =>
    message |> Message.reply("pong") |> Message.react("👍") |> ignore
  | _ => ignore()
  };
};

switch (token) {
| Some(token) => Discord.make(~onMessage, ~token) |> Lwt_main.run
| None =>
  print_endline("ERROR: No token found, try exporting DISCORD_BOT_TOKEN")
};