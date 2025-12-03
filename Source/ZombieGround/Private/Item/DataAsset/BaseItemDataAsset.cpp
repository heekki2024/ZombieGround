// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DataAsset/BaseItemDataAsset.h"

int32 UBaseItemDataAsset::GetSortPriority() const
{
	return static_cast<int32>(sortPriority);

}
