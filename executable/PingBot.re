open Lwt.Infix;

let token = Sys.getenv("DISCORD_BOT_TOKEN");

let onMessageHandler =
    (sendReact, sendMessage, message: ReDiscord.MessageHandler.message) => {
  switch (message.content) {
  | "ping" =>
    sendMessage("pong");
    sendReact("👍");
  | _ => Lwt.return_unit
  };
};

ReDiscord.Discord.make(~onMessageHandler, ~token) |> Lwt_main.run;