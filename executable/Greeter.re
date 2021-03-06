open ReDiscord;

let token = Sys.getenv_opt("DISCORD_BOT_TOKEN");

let onReady = () => {
  print_endline("Connected!");
};

let onGuildMemberAdd = (guild: Guild.t, member: GuildMember.t) => {
  let generalChannel =
    guild.channels
    |> List.find_opt((channel: Channel.t) => channel.name == "general");

  switch (generalChannel) {
  | Some(channel) =>
    channel |> Channel.send("Welcome, " ++ member.user.username)
  | _ => ignore()
  };
};

switch (token) {
| Some(token) =>
  Discord.make(~onGuildMemberAdd, ~onReady, token) |> Lwt_main.run
| None =>
  print_endline("ERROR: No token found, try exporting DISCORD_BOT_TOKEN")
};
