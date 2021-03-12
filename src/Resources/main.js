
const buffer = window.sharedBuffer = new Float32Array(4);

window.gameTick = (timeInMS) => {
    buffer[0] = Math.abs(Math.sin(timeInMS / 1000));
    buffer[1] = Math.abs(Math.cos(timeInMS / 1000));
    buffer[2] = Math.abs(Math.sin(timeInMS / 1000));
    buffer[3] = 1;
};
