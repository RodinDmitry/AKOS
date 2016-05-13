match($0, /\"\/[^"]*\"/)
{
	print substr($0, RSTART+1, RLENGTH-2)
}
