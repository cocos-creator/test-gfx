import { TestBase } from '../test-base';

import { ClearScreen } from './clear-screen';
import { BasicTriangle as reserved } from './basic-triangle';
import { BasicTriangle } from './basic-triangle-ref';

export const testlist: Constructor<TestBase>[] = [
    ClearScreen,
    reserved,
];
