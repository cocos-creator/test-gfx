import { TestBase } from '../test-base';

import { ClearScreen } from './clear-screen';
import { BasicTriangle } from './basic-triangle';
import { RotatingTriangle } from './rotating-triangle';
import { BasicSphere } from './basic-sphere';
import { CubeTexture } from './cube-texture';
import { EngineTest } from './engine-test';
import { DepthTest } from './depth-test';
import { JuliaSet } from './julia-set';

export const testlist: Constructor<TestBase>[] = [
    ClearScreen,
    BasicTriangle,
    RotatingTriangle,
    BasicSphere,
    CubeTexture,
    DepthTest,
    EngineTest,
    JuliaSet,
];
