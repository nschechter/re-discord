open Lwt.Infix;
open Util;

let make = (~onMessageHandler=?, ~token: string) => {
  Util.getDiscordGateway(~token)
  >>= (
    response =>
      switch (response) {
      | Error => Lwt.return_unit
      | Success(data) =>
        Uri.with_scheme(Uri.of_string(data.url), Some("https"))
        ->DiscordClient.make(onMessageHandler, token)
      }
  );
};