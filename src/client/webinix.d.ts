type B64string = string;

function webinix_fn(fn: string, value: string): Promise<void>; 
function webinix_log(status: boolean): void;
function webinix_encode(str: string): B64string;
function webinix_decode(str: B64string): string;