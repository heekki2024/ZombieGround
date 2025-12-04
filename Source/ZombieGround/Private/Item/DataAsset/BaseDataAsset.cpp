// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DataAsset/BaseDataAsset.h"


int32 UBaseDataAsset::GetSortPriority() const
{
	return static_cast<int32>(sortPriority);

}
