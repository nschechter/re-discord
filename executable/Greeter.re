open ReDiscord;

let token = Sys.getenv_opt("DISCORD_BOT_TOKEN");

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
  Discord.make(~onGuildMemberAdd, ~onReady, token) |> Lwt_main.run
| None =>
  print_endline("ERROR: No token found, try exporting DISCORD_BOT_TOKEN")
};