match($0, /href=\"[^"]+\">/)
{
	print substr($0, RSTART, RLENGTH)
}
