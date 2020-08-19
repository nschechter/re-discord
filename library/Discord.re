open Lwt.Infix;

let make =
    (
      ~onReady=?,
      ~onMessage=?,
      ~onMessageWithVoice=?,
      ~onGuildMemberAdd=?,
      ~onGuildMemberRemove=?,
      ~onReactionAdd=?,
      ~onReactionRemove=?,
      ~onVoiceConnect=?,
      ~debug=false,
      token: string,
    ) => {
  Api.getDiscordGateway(~token)
  >>= (
    response =>
      switch (response) {
      | Error =>
        print_endline("Unable to connect to the discord gateway")
        |> Lwt.return
      | Success(data) =>
        Uri.with_scheme(Uri.of_string(data.url), Some("https"))
        |> DiscordClient.connect(
             ~onReady,
             ~onMessage,
             ~onMessageWithVoice,
             ~onGuildMemberAdd,
             ~onGuildMemberRemove,
             ~onReactionAdd,
             ~onReactionRemove,
             ~onVoiceConnect,
             ~debug,
             ~token,
           )
      }
  );
};
