open Lwt.Infix;
open Websocket;
open Frame;

type state = {
  token: string,
  heartbeatInterval: option(int),
  userId: string,
  sessionId: string,
  serverId: string,
  ssrc: option(int),
  ip: string,
  port: option(int),
  modes: list(string),
};

let rec triggerHeartbeat = (interval, sendFrame) => {
  Lwt_timeout.create(
    interval,
    () => {
      sendFrame(
        Frame.create(
          ~opcode=Opcode.Text,
          ~content=PayloadGenerator.voiceHeartbeat(Random.int(99999999)),
          (),
        ),
      )
      |> ignore;
      triggerHeartbeat(interval, sendFrame);
    },
  )
  |> Lwt_timeout.start;
};

let sendVoicePacket = (sendFrame, packet) =>
  sendFrame(Frame.create(packet));

let startSpeaking = (sendFrame, ssrc, ~speaking=1) =>
  sendFrame(
    Frame.create(
      ~opcode=Opcode.Text,
      ~content=PayloadGenerator.voiceSpeak(~speaking, ssrc),
    ),
  );

let stopSpeaking = (sendFrame, ssrc, ()) =>
  sendFrame(
    Frame.create(
      ~opcode=Opcode.Text,
      ~content=PayloadGenerator.voiceSpeak(~speaking=0, ssrc),
    ),
  );

let handleEvent = (~state, ~sendFrame, ~onVoiceConnect, payload) => {
  payload
  |> PayloadParser.parseVoice
  |> (
    fun
    | VoiceHello(heartbeatInterval) => {
        triggerHeartbeat(heartbeatInterval / 1000, sendFrame);
        sendFrame(
          Frame.create(
            ~opcode=Opcode.Text,
            ~content=
              PayloadGenerator.voiceIdentify(
                ~serverId=state.serverId,
                ~userId=state.userId,
                ~sessionId=state.sessionId,
                state.token,
              ),
            (),
          ),
        )
        |> ignore;

        state;
      }
    | VoiceReady(payload) => {
        let readyData = VoiceReady.handle(payload.d);

        let newState = {
          ...state,
          ssrc: Some(readyData.ssrc),
          ip: readyData.ip,
          port: Some(readyData.port),
          modes: readyData.modes,
        };

        switch (onVoiceConnect) {
        | Some(handle) =>
          handle(
            newState,
            startSpeaking(~sendFrame, ~ssrc=readyData.ssrc),
            stopSpeaking(~sendFrame, ~ssrc=readyData.ssrc),
            sendVoicePacket,
          )
        | None => ignore()
        };

        newState;
      }
    | HeartbeatACK
    | _ => state
  );
};

let connect =
    (~debug, ~token, ~serverId, ~sessionId, ~userId, ~onVoiceConnect, uri) =>
  WebsocketClient.connect(
    ~debug,
    ~initialState={
      token,
      heartbeatInterval: None,
      userId,
      sessionId,
      serverId,
      ssrc: None,
      ip: "",
      port: None,
      modes: [],
    },
    ~handleFrame=
      (frame, sendFrame, state) =>
        (
          switch (frame.opcode) {
          | Opcode.Ping =>
            sendFrame(Frame.create(~opcode=Opcode.Pong, ())) |> ignore;
            state;
          | Opcode.Text
          | Opcode.Binary =>
            handleEvent(~state, ~sendFrame, ~onVoiceConnect, frame.content)
          | _ =>
            // TODO: Implement resume
            sendFrame(Frame.close(1002))
            >>= (() => Lwt.fail(Exit))
            |> ignore;
            state;
          }
        )
        |> Lwt.return,
    uri,
  );
