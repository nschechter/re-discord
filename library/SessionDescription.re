open Yojson.Basic.Util;

// TODO: secretKey bytes are 8 bit integers
type t = {
  mode: string,
  secretKey: list(int),
};

let extract = json => {
  mode: json |> member("mode") |> to_string,
  secretKey: json |> member("secret_key") |> convert_each(to_int),
};

let handle = payload => payload |> extract;
