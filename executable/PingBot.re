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

let onReady = () => {
  print_endline("Connected!");
};

let onGuildMemberAdd = (guild: Guild.t, member: Member.user) => {
  let generalChannel =
    guild.channels
    |> List.find_opt((channel: Channel.t) => channel.name == "general");

  switch (generalChannel, token) {
  | (Some(channel), Some(token)) =>
    Channel.send(~token, ~channel, "Welcome, " ++ member.username)
  | (_, _) => ignore()
  };
};

switch (token) {
| Some(token) =>
  Discord.make(~onGuildMemberAdd, ~onReady, ~onMessage, token) |> Lwt_main.run
| None =>
  print_endline("ERROR: No token found, try exporting DISCORD_BOT_TOKEN")
};