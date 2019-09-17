open Lwt.Infix;

let encodeMessage = message =>
  `Assoc([("content", `String(message)), ("tts", `Bool(false))])
  |> Yojson.Basic.to_string;

let createMessage = (~token: string, ~channelId: string, message: string) => {
  Cohttp_lwt_unix.Client.post(
    ~headers=
      Cohttp.Header.init_with("Authorization", "Bot " ++ token)
      ->Cohttp.Header.add("Content-Type", "application/json"),
    ~body=message |> encodeMessage |> Cohttp_lwt__.Body.of_string,
    Uri.of_string(
      Constants.http.baseApi
      ++ "/v"
      ++ Constants.http.version
      ++ "/channels/"
      ++ channelId
      ++ "/messages",
    ),
  )
  >>= (
    ((_resp, body)) => {
      Cohttp_lwt__.Body.to_string(body) >>= (body => body |> Lwt.return);
    }
  );
};

let createReact =
    (~token: string, ~channelId: string, ~messageId: string, emoji: string) => {
  Cohttp_lwt_unix.Client.put(
    ~headers=Cohttp.Header.init_with("Authorization", "Bot " ++ token),
    Uri.of_string(
      Constants.http.baseApi
      ++ "/v"
      ++ Constants.http.version
      ++ "/channels/"
      ++ channelId
      ++ "/messages/"
      ++ messageId
      ++ "/reactions/"
      ++ emoji
      ++ "/@me",
    ),
  )
  >>= (
    ((_resp, body)) => {
      Cohttp_lwt__.Body.to_string(body) >>= (body => body |> Lwt.return);
    }
  );
};