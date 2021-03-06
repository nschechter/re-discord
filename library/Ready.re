open Yojson.Basic.Util;

type t = {
  sessionId: string,
  user: Member.user,
};

let extract = (data: Yojson.Basic.t): t => {
  sessionId: data |> member("session_id") |> to_string,
  user: data |> member("user") |> Member.extractUser,
};

let handle = (_, payload, handler) => {
  switch (handler) {
  | Some(h) => h()
  | None => ignore()
  };

  extract(payload);
};
