open Lwt.Infix;

let token = "Mzg1NTUwNTk3MzE2NjczNTM3.XWLKvw.aptXh59bPNOHK7XY3xyk_MD0fgM";

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