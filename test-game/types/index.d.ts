declare namespace ion
{
	type EntryArgs = readonly string[];
	type Entry = (args: EntryArgs) => void;
}
