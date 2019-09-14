open Lwt.Infix;
open ReDiscord;

let token = Sys.getenv("DISCORD_BOT_TOKEN");

let onMessage = (message: Message.t) => {
  switch (message.content) {
  | "ping" =>
    message |> Message.reply("pong") |> Message.react("👍") |> ignore
  | _ => ignore()
  };
};

Discord.make(~onMessage, ~token) |> Lwt_main.run;