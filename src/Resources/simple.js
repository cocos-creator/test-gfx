
const { mat4, vec4 } = glMatrix;

const color = window.sharedBuffer = vec4.create();

window.gameTick = (timeInMS) => {
    vec4.set(color,
        Math.abs(Math.sin(timeInMS / 1000)),
        Math.abs(Math.cos(timeInMS / 1000)),
        Math.abs(Math.sin(timeInMS / 1000)),
        1
    );
};
