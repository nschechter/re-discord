open Lwt.Infix;

let make =
    (
      ~onReady=?,
      ~onMessage=?,
      ~onGuildMemberAdd=?,
      ~onGuildMemberRemove=?,
      ~onReactionAdd=?,
      ~onReactionRemove=?,
      ~debug=false,
      token: string,
    ) => {
  Api.getDiscordGateway(~token)
  >>= (
    response =>
      switch (response) {
      | Error => Lwt.return_unit
      | Success(data) =>
        Uri.with_scheme(Uri.of_string(data.url), Some("https"))
        |> DiscordClient.make(
             ~onReady,
             ~onMessage,
             ~onGuildMemberAdd,
             ~onGuildMemberRemove,
             ~onReactionAdd,
             ~onReactionRemove,
             ~debug,
             ~token,
           )
      }
  );
};